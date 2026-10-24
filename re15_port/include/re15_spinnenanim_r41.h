/*
 * RE1.5 Rebuilt — Runde 41 Spur L "spinnenanim": Zeichnen der Adultspinne (Typ 0x25) wie das Original.
 *
 * Nutzer-Punkt (analysis/befunde_runde41/AUFTRAG.md Z. 47):
 *   "Die Spinnen Animation beim herausklettern vom Gitter und runter klettern an der Wand stimmen noch
 *    nicht ganz."
 *
 * Dossier: analysis/befunde_runde41/L_spinnenanim.md. Die Choreografie selbst (Phasen @0x80114ec8, Runde 38
 * spinnen_r38.c) ist Bild fuer Bild gleich dem Original (Dossier 1.1); abweichend war das ZEICHNEN:
 *
 *   (1) Figur-Matrix. Das Original zeichnet jede Figur mit RotMatrix(+0x68 rot_x, +0x6a Gier, +0x6c rot_z)
 *       (FUN_8001e8c8 `jal 0x80068098` @0x8001e8f4, Ziel Figur+0x20) und skaliert sie bei Flag 0x800 mit
 *       +0x166 (@0x8001e904 `andi 0x800`, @0x8001e91c-938 `lh 358`, `jal 0x80065ff0` ScaleMatrix @0x8001e940).
 *       Die Spinne setzt im INIT Flag 0x800 (`ori v0,v0,0x800` @0x80110de8, `sw` @0x80110dec) und
 *       +0x166 = 0xccc (`ori v0,zero,0xccc` @0x80110df8, `sh v0,358` @0x80110dfc). Der Port zeichnete Typ
 *       0x25 nur mit der Gier und ohne Massstab: an der Wand (rot_z = 1024 @0x8011505c) lag sie waagrecht,
 *       und sie war 1/0.8 = 1,25-mal zu gross.
 *   (2) Gezeichnetes Bild. anim_set FUN_8001f314 waehlt das Bild mit +0x95 (`lbu v0,149` @0x8001f35c,
 *       `sw a2,360` @0x8001f36c), FUN_8001f3bc posiert DIESES Bild (Keyframe `jal 0x8001f664` @0x8001f534,
 *       Ueberblendung mit +0x8f VOR dem Herunterzaehlen: `mult` @0x8001f598, `addiu -1 / sb 143`
 *       @0x8001f5b0-b4) und zaehlt +0x95 ERST DANACH hoch (@0x8001f610-1c, Umbruch `sb zero,149` @0x8001f63c).
 *       Der Port-Zwilling (spinnen_r38.c lauer_anim, enemy_ai_common.c re15_aspider_anim) zaehlt vor dem
 *       Zeichnen hoch -> der Port zeigte jedes Bild einen Takt zu frueh, im Umbruchtakt Bild 0 statt 31
 *       (Haltepose statt Kopf-ueber-Pose beim Uebergang auf die Wand), und die Ueberblendung einen Schritt
 *       zu weit.
 */
#ifndef RE15_SPINNENANIM_R41_H
#define RE15_SPINNENANIM_R41_H

#include <stdint.h>
#include "re15_actor.h"

/* +0x166 = 0xccc (`ori v0,zero,0xccc` @0x80110df8 / `sh v0,358(v1)` @0x80110dfc, STAGE2.BIN). */
#define RE15_SPINNENANIM_R41_MASSSTAB 0x0ccc

/* Zustandshaken der Spinne (enemy_ai_common.c re15_adult_spider_ai_tick, vor dem Zustands-Dispatch, beide
 * KI-Modi): Flag 0x800 + Massstab 0xccc -> Port-Feld render_scale_q12 (main.c ScaleMatrix-Zwilling). */
void re15_spinnenanim_r41_init(re15_actor_t *e);

/* Zeichner (main.c): 1 = Figur-Matrix aus rot_x/Gier/rot_z (RotMatrix @0x8001e8f4) statt nur aus der Gier. */
int re15_spinnenanim_r41_volle_drehung(const re15_actor_t *e);

/* Figur-Matrix wie FUN_8001e8c8: RotMatrix(rot_x, rot_y, rot_z), bei gesetztem render_scale_q12 mit dem
 * Massstab multipliziert (ScaleMatrix @0x8001e940). m = 3x3 Q12, zeilenweise. Fuer jede Figur verwendbar;
 * Grundlage fuer den Test und den Gift-Anker (spinnen_r38.c). */
void re15_spinnenanim_r41_figur_matrix(const re15_actor_t *e, int32_t m[9]);

/* anim_set-Zwilling der Spinne: VOR dem Vorschub rufen. Merkt (Clip, Bild +0x95, Ueberblendung +0x8f) des
 * Takts, die FUN_8001f3bc posiert, und ob die Spinne dabei im Schacht-Ablauf (Zustand 4, +0x5 = 1) steht. */
void re15_spinnenanim_r41_vor_anim(const re15_actor_t *e);

/* Zeichner: Bild und Ueberblendzaehler, die dieser Takt posiert (Rueckgabe 1), solange die Spinne im
 * Schacht-Ablauf steht (inkl. INIT-Halten und Landetakt); sonst 0 = Port-Standard (anim_frame/anim_frac). */
int re15_spinnenanim_r41_zeichenbild(const re15_actor_t *e, int *bild, int *frac);

/* Zeichner-Klammer um re15_skel_compute_pose (main.c): beginn setzt *ncur (30-Bilder-Takt: Bild, 60: Bild>>1)
 * und den Ueberblendzaehler des gezeichneten Bilds, ende stellt anim_frac des Spielstands wieder her. */
void re15_spinnenanim_r41_zeichnen_beginn(re15_actor_t *e, uint32_t *ncur, int fps);
void re15_spinnenanim_r41_zeichnen_ende(re15_actor_t *e);

/* Test/Messschiene: alle Merker loeschen (Raumwechsel/Testanfang). */
void re15_spinnenanim_r41_reset(void);

#endif /* RE15_SPINNENANIM_R41_H */
