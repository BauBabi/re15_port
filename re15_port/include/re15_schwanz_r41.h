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

/* Runde 41 SF NB3: Szenenziel der Schwanzspitze (Welt) fuer Slot `slot` mit Gewicht Q12 (4096 = Spitze liegt auf dem
 * Ziel, FABRIK von der Wurzel; 0 oder spitze == NULL = aus). Ohne Ziel haengt jedes Glied hoechstens 62 Grad von der
 * Senkrechten ab (Labor-Pult: kein waagerechter Dorn). */
void re15_schwanz_r41_ziel(int slot, const int32_t spitze[3], int32_t gewicht_q12);

/* Runde 41 SF NB4 (Abnahme 2 A1/A2/A4): Fuehrung des Szenenziels. `via` (Welt, NULL = keiner) ist ein Durchgangspunkt fuer
 * Gelenk `via_j` (1..6): FABRIK in zwei Abschnitten Wurzel -> via -> Spitze. `beuge_q12` (Welt, Q12, NULL = aus) laesst die
 * Rechnung an einer in diese Richtung gebogenen Startform beginnen statt am Federstand; die Szene gibt die Blickrichtung der
 * Kamera, damit ueberschuessige Laenge in der Tiefe (hinter dem Koerper) liegt. Gilt nur, solange ein Ziel gesetzt ist. */
void re15_schwanz_r41_fuehrung(int slot, const int32_t via[3], int via_j, const int32_t beuge_q12[3]);

/* Runde 41 SF NB5 (Abnahme 3 B1): Dicke der gezeichneten Glieder 16-22 (Q12, 4096 = Modell) fuer Slot `slot`; skaliert nur
 * den Querschnitt (lokale x/z-Achsen), Gliedlaengen und Kette bleiben. 0 = 4096. */
void re15_schwanz_r41_dicke(int slot, int32_t dicke_q12);
/* Runde 41 SF NB5 (Abnahme 3 B1): die ersten `glieder` Glieder ab der Wurzel werden ohne Querschnitt gezeichnet (verdeckt - im
 * Video liegt das Stueck Huefte -> Austritt hinter dem Koerper); Kette und Lage bleiben. 0 = alle sichtbar. */
void re15_schwanz_r41_verdeckt(int slot, int glieder);

/* Runde 41 SF V8 Schwanz-Bewegung (Bauplan 3, PORT-SZENE nach Video 4): Rolle der Glieder 16-22 um ihre Achse; `punkt` = Szenenkamera
 * (Welt). modus 1: die Bauchseite (+e_h, beim Laden aus der Flachachse der Meshes 17-21 gemessen) zeigt quer zur Gliedachse zur Kamera
 * (Schmalseite); modus 2: quer zu Gliedachse und Sehstrahl zur Mitte der Sehne Wurzel -> Spitze (Innenseite des Bogens, Flanke zur Kamera).
 * Hoechstens 30 Grad Verdrehung gegen das Vorglied (ausser hinter einem verdeckten Glied); Kette und Gelenkwinkel bleiben. gewicht Q12,
 * 0 = aus. Nur V8-Kette. */
void re15_schwanz_r41_rolle(int slot, int modus, const int32_t punkt[3], int32_t gewicht_q12);
/* Runde 41 SF V8 Schwanz-Bewegung (Bauplan 6): 1 = Hangar-Szene - unter Szenenfuehrung knickt die V8-Kette bis 39,4 Grad je Gelenk
 * (ueberblendet mit dem Gewicht des Szenenziels, auch in FABRIK); ohne Szenenziel und mit 0 gelten die Kampfwerte unveraendert. */
void re15_schwanz_r41_szene(int slot, int an);
/* Runde 41 SF V8 Schwanz-Bewegung (Bauplan 5): Bauch der Startform des Szenenziels IN DER BILDEBENE - Richtung bild_n (Welt, Q12, quer zur
 * Sehne im Bild) mit hoechstens `pfeil` Welteinheiten (Pfeil der Videolinie); der Rest des Ueberschusses liegt in der Beugerichtung von
 * re15_schwanz_r41_fuehrung (Blickrichtung). NULL / 0 = aus. */
void re15_schwanz_r41_bogen(int slot, const int32_t bild_n_q12[3], int32_t pfeil);
/* Runde 41 SF V8 Schwanz-Bewegung (Bauplan 5): die Spitze des Szenenziels gleitet auf dem Sehstrahl von `kamera` (Welt) so weit, dass die
 * Kette ab dem Austritt mit dem Pfeil der Bildebene genau hinreicht (Bildlage der Spitze bleibt). NULL = aus. Nur V8 unter Szenenfuehrung. */
void re15_schwanz_r41_tiefe_frei(int slot, const int32_t kamera[3]);

/* Zuruecksetzen (Raumwechsel, Tests). */
void re15_schwanz_r41_reset(void);

/* V4 Runde 3: 1 = Skelett traegt die V4-Kette (Gliedlaengen max/min > 2,0, schwanz_r41.c SW_GLATT_V4_VERH). */
int  re15_schwanz_r41_v4_kette(const re15_emd_skeleton_t *s);

/* V8 (Messblatt V8 6a): 1 = Skelett traegt die V8-Kette (gleiche Glieder >= 500 E., Wurzel oben am Ruecken), schwanz_r41.c SW_V8_*. */
int  re15_schwanz_r41_v8_kette(const re15_emd_skeleton_t *s);

/* Messzugang fuer Tests: Gelenk j (0 = Wurzel .. 7 = Spitze) der Kette von Slot `slot` in Weltkoordinaten. */
int  re15_schwanz_r41_gelenk(int slot, int j, int32_t out[3]);
/* V4 Runde 3: Messzugang - Feder-Ziel (Keyframe-Gelenk j der gezeichneten Pose, Welt) des letzten Kettenschritts. */
int  re15_schwanz_r41_ziel_gelenk(int slot, int j, int32_t out[3]);

#endif
