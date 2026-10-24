#ifndef RE15_BOSS_GATOR_H
#define RE15_BOSS_GATOR_H

/* ROOM2090 Alligator-BOSSKAMPF — NUTZER-DESIGN (Auftrag 2026-09-10), KEIN Original-
 * Nachbau: "Der laeuft aber etwas anders ab als im Original." Die BAUSTEINE sind die
 * byte-true Systeme (0x23-KI-Mechanik @0x80118bc8, re15_enemy_take_damage,
 * re15_esp_fx_spawn, EM23-RE2-Bank); die ABLAUF-Konstanten dieses Kampfs sind
 * deklarierte DESIGN-Werte und als solche kommentiert. Die byte-true
 * re15_alligator_ai_tick bleibt unveraendert bestehen (andere Raeume). */

#include "re15_actor.h"

/* 1, wenn dieser Aktor im aktuellen Raum als Boss laeuft (Typ 0x23 in ROOM2090/2091). */
int  re15_gator_boss_active(const re15_actor_t *e);

/* Der Boss-Tick — ersetzt re15_alligator_ai_tick fuer den Boss-Aktor. */
void re15_gator_boss_tick(int slot);

/* 1 = der Dispatcher klemmt diesen Aktor NICHT. Runde 41 Spur B, Nachbesserung 4: der Boss klemmt sich am Ende seines
 * eigenen Takts selbst (bitgleiche Rechnung, ausgesetzt waehrend der Plattform-Ueberquerung und der Selbst-Befreiung), damit
 * der Kanister im Maul die Endlage des Bildes sieht - fuer einen aktiven Boss also immer 1. */
int  re15_gator_boss_skip_clamp(const re15_actor_t *e);

/* Wirbelsaeulen-Bogen (Nutzer-Punkt 8): zusaetzlicher vz-Winkel (Q12) fuer den
 * Skelett-Bone `bone` des Boss-Aktors; 0 wenn inaktiv. Wird von
 * re15_skel_compute_pose fuer Typ-0x23-Aktoren abgefragt. */
int  re15_gator_spine_arc_vz(const re15_actor_t *e, int bone);

/* 1 solange die Fress-Sequenz laeuft - der Gameover-Exit (game_step_common)
 * haelt dann an, bis Schnapp 2 + Verschlingen sichtbar durchgespielt sind. */
int  re15_gator_fressen_hold(void);
/* RE2-Todes-Latch: laeuft der Fress-Finisher ab dem f13-Koppelframe? Ab da gehoert im
 * Original die Todes-Praesentation dazu (s. enemy_ai_boss_gator.c). */
int  re15_gator_fress_todeslatch(void);
/* ENEMSE-Anbindung (Bank 17, Dossier gator-biss-sound.md) - PC registriert beim
 * 0x23-Laden; se_fn(id, flag2000), bank_fn(bank). */
void re15_gator_audio_hook(void (*se_fn)(int, int), void (*bank_fn)(int));
/* Runde 41 Spur B, Nachbesserung 3 (Abnahme 2 M1, Orchestrator-Entscheid B9): 1 = der Boss ist vor seinem Durchbruch
 * UNTERGETAUCHT wie RE2 (Sce_em_set y = +3000 @RDT 0x25A0 ROOM40A0, Pos_set erst im Intro @SCD 0x0736): nicht zeichnen
 * (Mesh und Schatten), keine Trefferbox. Auch 1 im Spawn-Bild vor dem Boss-INIT (state 0). Fuer Spur F/G: die Szene
 * "Alligator springt aus dem Wasser" taucht ihn mit re15_gr2_intro_start auf. */
int  re15_gator_boss_untergetaucht(const re15_actor_t *e);
/* Runde 41 Spur B, Nachbesserung 4 (Abnahme 3, M2) - Messhaken: Zahl der Selbst-Befreiungen (nur noch im echten Gefaengnis, als
 * Schub ohne Gier-Snap) und 1 = Boss in der Verfolgung (CHASE) oder Belagerung (GUARD). */
int  re15_gator_boss_befreiungen(void);
int  re15_gator_boss_verfolgt(const re15_actor_t *e);
int  re15_gator_boss_phase(const re15_actor_t *e);   /* Messhaken: Boss-Phase (2 = CHASE, 5 = GUARD, 7 = CROSS; -1 = kein Boss) */
int  re15_gator_boss_im_fressen(const re15_actor_t *e);   /* Nachbesserung 5: 1 = Fress-Finisher (FRESSEN P0-P3) laeuft */
/* Messhaken GIER-WACHE (NB4, M2): Bildpaare mit beiden Takten in CHASE/GUARD, groesstes |Gier-Delta| (ohne den RE2-Knick +1024),
 * Zahl der Deltas > 0x60 (Port-Wenderate). gator_boss.log: "GIER-SPRUNG ..." je Verstoss, "GIER-WACHE ..." alle 300 Paare. */
int  re15_gator_boss_gier_wache(int slot, int *paare, int *max_delta, int *spruenge);



#endif /* RE15_BOSS_GATOR_H */
