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
/* RE15_GATOR_ORIGINAL=1: der RE1.5-Alligator statt des Bosskampf-Designs — Modell CDEMD0.EMS Block 25 (Sektor 1922,
 * im Auslieferungsstand ohne Eintrag in der Modell-Ladetabelle @0x80072f38, Typ 0x23 = 0/0) und die RE1.5-KI der
 * Wurzel 0x8010c448 (STAGE2, registriert @0x80116f64; Port re15_alligator_ai_tick). */
int  re15_gator_original(void);

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
/* Runde 41 Spur P (Nachbesserung 1, M2): 1 = RE2-R1 hat begonnen, der Koerper schiebt niemanden mehr (R1 P0 +0 |= 2 @0x801022f8-fc, Gate FUN_80034d0c). */
int  re15_gator_boss_ohne_koerper(const re15_actor_t *e);
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
/* Runde 41 Spur G2-RE2 (Ueberquerung mit dem Kanister, Rammen; aus r41/gatorfix NB7-NB9): Pruefhaken der Riegel (Gegenproben, kein Spielweg) und Messhaken. */
void re15_gator_boss_ramm_alt_setzen(int an);   /* 1 = Ueberquerung mit dem Kanister ohne Rammen */
void re15_gator_boss_deck_alt_setzen(int an);   /* 1 = Push ohne Halt auf der begehbaren Rampe */
void re15_gator_boss_kdt_alt_setzen(int an);    /* 1 = Ueberquerung ohne Hub ueber dem Deck */
int  re15_gator_boss_rammen(void);              /* Rammen seit Programmstart */
int  re15_gator_boss_kreuz_kanister(void);      /* Ueberquerungen mit dem Kanister seit Programmstart */
int  re15_gator_boss_koerper_kontakt(const re15_actor_t *e, int32_t px, int32_t pz);   /* Punkt im Koerper-Grundriss (Push-Kreise + 450) */
void re15_gator_boss_ziel(const re15_actor_t *e, int32_t *tx, int32_t *tz);   /* Messhaken: Steuerziel des letzten CHASE-Takts */
int  re15_gator_boss_lauert(const re15_actor_t *e);   /* Messhaken: Takt lauerte am Startpunkt der Ueberquerung */
void re15_gator_boss_tempo_alt_setzen(int an);  /* Pruefhaken: 1 = RE1.5-Tempo B4 statt RE2-Vortrieb (Gegenprobe "Gang gleich NB5") */
/* Runde 41 Spur G2-RE2 NB3 (Nutzerentscheid 2026-11-06, Vorgabe A + B): Pruefhaken 1 = Stand a661eb8aa (Gegenproben der Riegel nb3); Messhaken
 * Bilder, die die Figur oben/in der Ostnische steht, ohne Treffer (GUARD quert ab GB_OBEN_OHNE 300 ueber sie). */
void re15_gator_boss_nb3_alt_setzen(int an);
int  re15_gator_boss_nb3_alt(void);
/* Runde 41 Spur G2-RE2 NB4 Runde 2 (Abnahme 7 M1): Pruefhaken 1 = Stand cfa775ae8 - LUNGE, Biss B2 und Rammen fragen die RE2-Bisssperre +0x238 nicht
 * (Gegenprobe Riegel nb4 'Zweitbiss'); im Spiel nicht gesetzt. */
void re15_gator_boss_sperre_alt_setzen(int an);
int  re15_gator_boss_oben_ohne(const re15_actor_t *e);
/* Runde 41 Spur G2-RE2 NB3, Runde 2 (Abnahme 5 M1/M2): RAUMHALT der Figur in der Spieler-Klemme waehrend ein Alligator-Boss im Raum ist (oben: Deck der
 * Rampe, unten: innerhalb der Aussenwaende, nicht in Plattform-/Rampenblock; Herleitung enemy_ai_boss_gator.c). (*x,*z) = geklemmte Lage, alt = Lage vor
 * dem Schritt; Rueckgabe 1 = eingegriffen. Pruefhaken 1 = Stand 10603425b (kein Halt), Messhaken Eingriffe seit Programmstart. */
int  re15_gator_boss_raum_halt(const re15_actor_t *pl, int32_t alt_x, int32_t alt_z, int32_t *x, int32_t *z);
void re15_gator_boss_raum_alt_setzen(int an);
int  re15_gator_boss_raum_halt_zahl(void);
void re15_gator_boss_takt_schritt(int32_t *dx, int32_t *dz);   /* Messhaken: Takt-Schritt des letzten Boss-Takts vor den Klemmen */
void re15_gator_boss_zeit_letzt(int64_t *ns, int64_t *zyk);    /* Messhaken: Wanduhr ns / Faden-Zyklen des letzten Boss-Takts (PC) */
/* gator_zeit_r41.c (aus r41/gatorfix NB9, Messhaken): Wanduhr ns, Faden-Zyklen (Windows QueryThreadCycleTime), Zyklen je us; Zeile je Takt in gator_zeit.log
 * bei env RE15_G2_ZEIT_LOG */
int64_t re15_g2_zeit_ns(void);
int64_t re15_g2_faden_zyk(void);
double  re15_g2_zeit_zyk_je_us(void);
void    re15_g2_zeit_takt(unsigned bild, int phase, int modus, int64_t takt_ns, int64_t takt_zyk, int64_t biege_ns, int64_t biege_zyk, const int64_t *teile);



#endif /* RE15_BOSS_GATOR_H */
