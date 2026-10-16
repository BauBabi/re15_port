/* re15_szenenschutz_r38.h — Runde 38 Spur B "szenenschutz" (AUFTRAG.md 2026-10-16: "Das Leon schon in Cutscenes
 * angegriffen werden kann, wo er sich noch nicht mal bewegen kann ist mist. So wie bei Leon zum Beispiel bei ROOM 1190
 * bei den Hunden oder 11D0 bei den Hunden. Es sei den es ist Teil der Cutscene wie bei Elza Room 1170.").
 * Dossier analysis/befunde_runde38/B_szenenschutz.md (Messung, Belege, Umsetzung).
 *
 * ZWEI TEILE:
 *
 * (A) SZENENSCHUTZ — NUTZER-VORGABE, Form aus dem Original. Das Original kennt keine allgemeine Sperre "Ereignis ->
 *     kein Angriff" (Set(2,7) = DAT_800aca40 Bit 0x01000000 liest nur die Pad-Aufbereitung @0x800304f4-0x80030500 und
 *     die Statusmenue-Sperre @0x8001cd0c-0x8001cd20; die Gegner-Wurzeln pruefen nur Bit 0x20000000, Hund @0x8010d80c-
 *     0x8010d818). Es kennt aber das Tor "Spieler gerade nicht angreifbar", das JEDER freie Angriff abfragt:
 *       RE1.5: Spieler+0x93 (0x800acae7) — Hund ATTACK-RANGE @0x8010e5c4-0x8010e5cc (statt Biss Knurren, Sub 6),
 *              Biss-Kontakt FUN_8010f15c (acae7 == 0), Schadens-Aufloeser FUN_80012d60 @0x80012e24;
 *       RE2:   PL+0x1D3 & 0x80 — Hund-Entscheid @0x80104D84-0x80104D90 (-4, Halbkreis P2 @0x80100E4C),
 *              Biss-Kontakt @0x80104E5C-68 (0), Zombie Block G @0x80102128, Spinne @0x80105A48-54 (Port:
 *              re15_player_is_grabbed()).
 *     Solange die Szene den Spieler haelt (Port: g_scd.player_mode == 2, einziger Schreiber platform/pc/main.c
 *     cine_active = flag(1,27) || flag(2,7) — das ist woertlich "Leon kann sich nicht bewegen"), sieht jeder Gegner
 *     in seinem FREIEN Gehirn (Zustand +0x4 == 1, ACTIVE — Wurzeltabellen [1]: Hund 0x8010dbcc, Kraehe 0x80112420,
 *     Zombie 0x80101224, RE2-Hund 0x801004DC) den Spieler durch genau diese Tore als nicht angreifbar.
 *     AUSNAHME "Teil der Szene": Skript-Zustaende (+0x4 != 1) bleiben unberuehrt — der Kraehen-Griff der 1171-Szene
 *     laeuft in FLIGHT-2 (+0x4 = 4, INIT-Weiche grid&0x40 @0x80112408) ueber @0x80115254-0x801152c4 OHNE +0x93-Tor;
 *     dazu die Kraehe mit noch gesetzter Skript-Marke grid&0x40 (dieselbe INIT-Weiche) in jedem Gehirn.
 *     SICHERHEITSNETZ: zieht ein geschuetzter Gegner dem Spieler trotzdem HP ab (ungatterter Pfad), bekommt der Spieler
 *     sie im selben Durchlauf zurueck — "kein Treffer, solange die Szene ihn haelt" (Protokoll "[szenenschutz]").
 *
 * (B) FREIGABE-SPERRZEIT 1190 — byte-true Uebernahme in das RE2-Hundegehirn. Der RE1.5-Sprung-Ausgang FUN_80111398
 *     Phase 4 schreibt +0x1d6 = 0x78 (@0x801115e8 `ori v0,zero,0x78` / @0x801115ec `sh v0,470(v1)`); das ACTIVE-
 *     Gehirn zieht ab (@0x8010dd5c-0x8010dd70) und beisst erst bei 0 (@0x8010e578-0x8010e580). Der Port fuehrt den
 *     Ausgang auch unter RE2 (enemy_ai_common.c re15_dog_state456: dog_atk_cd = 0x78), das RE2-Gehirn las den Wert nie
 *     -> Biss 13 Bilder nach Szenenende. Hier: Angriffs-Commits des RE2-Hundes (STALK @0x801009C8, RUN P1 ueber den
 *     Entscheid @0x80100D88, RUN P3 @0x8010119C) ruhen, solange dog_atk_cd != 0; Abzug je ACTIVE-Bild.
 */
#ifndef RE15_SZENENSCHUTZ_R38_H
#define RE15_SZENENSCHUTZ_R38_H

#include <stdint.h>
#include "re15_actor.h"

/* ---- (A) Szenenschutz: Haken in re15_enemy_ai_run_all (enemy_ai_common.c) ---------------------------------- */
void re15_szenenschutz_durchlauf_beginn(void);   /* Kopf des KI-Durchlaufs                         */
void re15_szenenschutz_gegner(int slot);         /* vor dem Tick JEDES aktiven Akteurs             */
void re15_szenenschutz_durchlauf_ende(void);     /* nach der Schleife                              */
int  re15_szenenschutz_sicht(void);              /* 1 = der gerade tickende Gegner sieht PL+0x1D3&0x80 */

/* Die Szene haelt den Spieler (g_scd.player_mode == 2). */
int  re15_szenenschutz_haelt(void);
/* Gilt der Schutz gegen diesen Akteur? (freies Gehirn, keine Szenen-Rolle) */
int  re15_szenenschutz_gegen(const re15_actor_t *e);

/* Zaehler fuer Tests/Protokoll: geschuetzte Gegner-Ticks, vom Sicherheitsnetz zurueckgegebene HP-Abzuege. */
void     re15_szenenschutz_zaehler(uint32_t *ticks, uint32_t *abgefangen);
void     re15_szenenschutz_zaehler_reset(void);

/* ---- (B) Freigabe-Sperrzeit: Haken im RE2-Hundegehirn (enemy_ai_re2_dog.c) --------------------------------- */
int  re15_r38_hund_sperrzeit(const re15_actor_t *e);   /* 1 = kein Angriffs-Commit (dog_atk_cd != 0)      */
void re15_r38_hund_sperrzeit_takt(re15_actor_t *e);    /* Abzug je ACTIVE-Bild (@0x8010dd5c-0x8010dd70)   */

#endif
