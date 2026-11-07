/*
 * re15_gator_treffer_r41.h - Runde 41 Spur G2-RE2 NB4 (T)+(W): die Trefferreaktion der Figur auf JEDEN Alligator-Angriff wie RE2,
 * mit dem Stoss von der Rampe ins Wasser. Dossier analysis/befunde_runde41/G2_gatorfix.md Abschnitt "G2-RE2 NB4", Messblatt
 * analysis/befunde_runde41/G2_MESSBLATT_NB4.md (Abschnitte 1.1, 2.3, 2.4, 5), Herleitung je Konstante in engine/src/gator_treffer_r41.c.
 *
 * RE2 (info/re2leon/COMMON/BIN/EM23_OVL_0000.BIN, Basis 0x80100000; Auszug G2_belege/nb4/mess_re2_em23_treffer_kanister.dis):
 *   Treffer-Einstieg 0x80101578 -> PL+4 = 0x205 (Spieler-Routine 5, Unterzustand 2) bzw. 3, wenn FUN_80015910(Gator, Figur);
 *   FUN_8001569c(Figur, Achse, 1536); Schaden FUN_800401d4 (60 / 30); Spieler-Haken 0x80102D08 Tafel @0x80104690 [2] 0x8010307C, [3] 0x801031F8.
 */
#ifndef RE15_GATOR_TREFFER_R41_H
#define RE15_GATOR_TREFFER_R41_H

#include <stdint.h>
#include "re15_actor.h"
#include "re15_rdt.h"
#include "re15_emd.h"

/* Treffer-Einstieg (aus gb_biss_abschluss, enemy_ai_boss_gator.c). geschwaecht = Gator +0x218 & 0x20 (re15_gr2_geschwaecht_slot).
 * Rueckgabe = FUN_800401d4: 0 ueberlebt, 1 One-Save (PL+6 = 4), 2 tot (PL+4 = 3; der Aufrufer faehrt das Fressen). */
int  re15_gator_treffer_r41_einstieg(const re15_actor_t *gator, re15_actor_t *pl, int geschwaecht, int art);
int  re15_gator_treffer_r41_aktiv(void);
/* Ein Bild des Spieler-Hakens 0x80102D08 (game_step_common.c, eigener Zweig vor dem Niederwurf); danach laeuft der Dispatcher-Schwanz
 * (Koerper-Schub + Klemme). pl00 = die Basisbank der Figur (Basis-Clip 6 von RE2 = RE1.5 PL00 Clip 11). */
void re15_gator_treffer_r41_tick(const re15_rdt_t *rdt, re15_actor_t *pl, const re15_emd_animation_t *pl00);
void re15_gator_treffer_r41_reset(void);   /* Kommandowort-Reset (re15_player_cmd_zero) */

/* Zeichner: >= 0 = die Figur posiert aus der RE2-Raumbank (GATOR40A0.RBJ Record 0) mit diesem Clip (Latch bis +0x94 neu geschrieben
 * wird, wie re15_player_react_pose_pl00); re15_gator_treffer_r41_pl00 = 1 solange der Schluss (Basis-Clip 6 rueckwaerts) laeuft. */
int  re15_gator_treffer_r41_rbj_clip(void);
int  re15_gator_treffer_r41_pl00(void);
/* Die Raumbank: Plattform haengt den Leser ein (PC: shared_assets/RE2/GATOR40A0.RBJ, Tests: dieselbe Datei). basis = Knochen der Figur. */
extern const uint8_t *(*re15_gator_treffer_r41_rbj_hook)(int *size);
int  re15_gator_treffer_r41_bank(const re15_emd_skeleton_t *basis, const re15_emd_skeleton_t **skel, const re15_emd_animation_t **anim);

/* Pruefhaken der Gegenproben: 1 = Stand 3d57083b1 (RE1.5-Niederwurf + Schaden-Tafel [5] = 50). */
void re15_gator_treffer_alt_setzen(int an);
int  re15_gator_treffer_alt(void);

/* Messung (Riegel T1-T3/W1): Zustand nach dem letzten Takt. */
typedef struct {
    int     aktiv, sub, ph, bild;          /* Unterzustand 2/3, Phase PL+6, Bilder seit Ph0 */
    int     clip, pose_f, raumbank;        /* gezeichneter Clip/Bild, 1 = RE2-Raumbank, 0 = Basisbank (rueckwaerts) */
    int     sperre;                         /* PL+0x1D3 & 0x80 */
    int     schritt;                        /* +0x144 dieses Bildes (Vorschub FUN_800152c8), 0 = kein Vorschub */
    int     fall;                           /* 0 kein Sturz, 1 faellt, 2 gelandet */
    int     n_start, n_ende, n_fall, n_landung;
    int     art, achse, schaden, ret, se;   /* Angriff (1 Rammen, 2 CROSS-Biss, 3 Hochbiss, 4 Biss, 5 Biss B2), Achse a, Schaden, FUN_800401d4 */
    int32_t start_x, start_z;               /* Lage im Treffer-Bild */
    int     gier_figur_vor, gier_gator;     /* Gieren im Treffer-Bild (vor FUN_8001569c) - Gegenrechnung FUN_80015910 */
} re15_gtr_info_t;
void re15_gator_treffer_r41_info(re15_gtr_info_t *o);

#endif /* RE15_GATOR_TREFFER_R41_H */
