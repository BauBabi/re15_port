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

/* Waehrend der Plattform-Ueberquerung darf der SCA-Wand-Clamp den Gator nicht vom
 * Plattform-Block stossen: 1 = Clamp fuer diesen Aktor aussetzen. */
int  re15_gator_boss_skip_clamp(const re15_actor_t *e);

/* Wirbelsaeulen-Bogen (Nutzer-Punkt 8): zusaetzlicher vz-Winkel (Q12) fuer den
 * Skelett-Bone `bone` des Boss-Aktors; 0 wenn inaktiv. Wird von
 * re15_skel_compute_pose fuer Typ-0x23-Aktoren abgefragt. */
int  re15_gator_spine_arc_vz(const re15_actor_t *e, int bone);



#endif /* RE15_BOSS_GATOR_H */
