/* re15_bissarm_r37.h — Runde 37 Spur J "bissarm" (Dossier analysis/befunde_runde37/J_bissarm.md).
 *
 * Nutzer (AUFTRAG.md Nachtrag 6): "fehlt in unseren Port teilweise noch der dranhängende Arm wenn man vom
 * Zombie gebissen wird." Zwei gemessene Ursachen, beide Port-Abweichungen vom RE2-Original:
 *
 * (A) HANDKONTAKT IM GRIFF. Der RE2-Flavor zeichnet den Hybrid (RE2-Rig mit RE1.5-Bindlaengen + root_y_fix,
 *     re2_ems.c re2_hybrid_apply). Die RE1.5-Zombies sind ~10 % kleiner (Beinkette 1675 gegen 1843, Oberarm 517
 *     gegen 569, Unterarm 419 gegen 459), die RE2-Griff-Clips legen die Hand des RE2-Zombies aber UEBER Leons
 *     Schulter (RE2-RAM SLUS-00748_4.sav, P3 @0x8010F838: Hand 11 45..180 von Leons Schultergelenk). Im Hybrid
 *     landet dieselbe Hand ~200 tiefer, unter der Schulter -> der Unterarm verschwindet hinter/in Leon.
 *     re15_bissarm_r37_handkontakt() stellt fuer die Dauer des Griffs (RE2-Exec[3] = +0x5 == 3, P0..P5) die
 *     Hand jedes Arms auf die Stelle, an der sie das RE2-Rig mit DEMSELBEN Keyframe zeichnet (RE2-Bindtabelle,
 *     keine Wurzelkorrektur), Ellbogen per Zwei-Gelenk-Kette mit den RE1.5-Armlaengen, Beugeseite = RE2-Ellbogen.
 *     PORT-KONSTRUKT wie root_y_fix (der Hybrid hat kein Original): die Invariante des Originals ist der
 *     Kontaktpunkt Hand–Opfer, er wird RE2-genau gehalten; Groesse und Aussehen der RE1.5-Modelle bleiben.
 *
 * (B) WANDKLEMME DER OPFER-PLATZIERUNG. RE2 klemmt den Spieler im Griff genau EINMAL je Bild gegen die Waende,
 *     NACH der Platzierung und vom Standpunkt des Vorbilds aus: Hauptschleife `jal 0x8003bfac` (Spieler-Tick,
 *     darin die Opfer-Platzierung 0x80015CB8) @0x80026620 -> `jal 0x800355c4` (Schub) @0x80026628 ->
 *     `jal 0x8003567c` @0x80026634; FUN_8003567c: `andi v0,v1,0x8 / bne` @0x80035694-98 und `lw v0,484(s0) / bne`
 *     @0x800356a0-a8 (beide im Griff 0: RAM word0 0x40001001, +0x1E4 = 0), `jal 0x8004c1bc` @0x800356d0
 *     (Radius +0x90 = 450, Band 1<<+0x106), danach alter Standpunkt `sh v0,280(s0)` / `sh v1,282(s0)`
 *     @0x800356f4-f8. Der Port nahm dafuer einen Startwert, den der Zombie-Griff nie setzte (ROOM1010: Leon den
 *     ganzen Griff auf (0,0)), und hielt Leon bei 2 Einheiten Rundungsrest auf dem letzten Standpunkt fest (bis
 *     394 neben der Formation). re15_bissarm_r37_ein_durchgang() meldet die Greifer, fuer die der eine
 *     Durchgang des Originals gilt (Zombie-Familie; RE1.5 ebenso: Spieler-Tick `jal 0x8003b0a4` @0x80031d70
 *     nach dem Kommando-Handler @0x80031cb4). */
#ifndef RE15_BISSARM_R37_H
#define RE15_BISSARM_R37_H
#include <stdint.h>
#include "re15_actor.h"
#include "re15_emd.h"
#include "re15_skeleton.h"

/* re2_ems.c re2_hybrid_apply: die RE2-Bindtabelle des Typs merken, BEVOR der Hybrid sie mit RE1.5-Laengen
 * ueberschreibt (Quelle RE2/CDEMD0.EMS, EMR des Typs). */
void re15_bissarm_r37_re2_bind_merken(int kind, const re15_emd_skeleton_t *re2skel);
int  re15_bissarm_r37_re2_bind_da(uint8_t type);

/* enemy_ai_common.c re15_victim_place: 1 = fuer diesen Greifer gilt EIN Wandklemm-Durchgang je Bild ohne
 * Rueckfall (RE2 FUN_8003567c, RE1.5 @0x80031d70). */
int  re15_bissarm_r37_ein_durchgang(uint8_t greifer_typ);

/* platform/pc/main.c NPC-Zeichner, direkt nach re15_skel_compute_pose: Arme des greifenden Hybrid-Zombies auf
 * den RE2-Handkontakt stellen. Liefert 1, wenn die Pose veraendert wurde. `kf` = der gezeichnete Keyframe. */
int  re15_bissarm_r37_handkontakt(const re15_actor_t *e, const re15_emd_skeleton_t *sk, int kf,
                                  re15_skel_pose_t *poses, int n);

/* Mess-/Pruefzugang (Tests): die Hand-Zielpunkte (Modellraum) des letzten Aufrufs und der Hand-Rest-
 * abstand (0 = erreicht, > 0 = Arm gestreckt, Ziel jenseits der RE1.5-Armlaenge). */
void re15_bissarm_r37_letzte_ziele(int32_t ziel[2][3], int32_t rest[2]);

#endif
